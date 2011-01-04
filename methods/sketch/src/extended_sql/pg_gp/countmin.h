#ifndef _COUNTMIN_H_
#define _COUNTMIN_H_
#define LONGBITS (sizeof(int64)*CHAR_BIT)
#define RANGES LONGBITS
#define DEPTH 8 /* magic tuning value: number of hash functions */
/* #define NUMCOUNTERS 65535 */
#define NUMCOUNTERS 1024  /* another magic tuning value: modulus of hash functions */

#ifndef MIN
#define MAX(x,y) ((x > y) ? x : y)
#define MIN(x,y) ((x < y) ? x : y)
#endif

/*! countmin is defined over postgres int8 type.  Should probably use the max of that type, not LONG_MAX */
#define MAXVAL (LONG_MAX >> 1)
/*! Midpoint is 1/2 of MAX .. i.e. shift MAX right. */
#define MIDVAL (MAXVAL >> 1)
#define MINVAL (LONG_MIN >> 1)

/*! 
 * a CountMin sketch is a set of DEPTH arrays of NUMCOUNTERS each.
 * It's like a "counting Bloom Filter" where instead of just hashing to
 * DEPTH bitmaps, we count up hash-collisions in DEPTH counter arrays
 */
typedef int64 countmin[DEPTH][NUMCOUNTERS];

/*!
 * the transition value struct for the cmsketch aggregate.  Holds the sketch counters
 * and a cache of handy metadata that we'll reuse across calls
 */
typedef struct {
    Oid typOid;     /*! oid of the data type we are sketching */
    Oid outFuncOid; /*! oid of the OutFunc for that data type */
    countmin sketches[RANGES];
} cmtransval;

/*! base size of a cmtransval */
#define CM_TRANSVAL_SZ (VARHDRSZ + sizeof(cmtransval))

/*!
 * a data structure to hold the constituent dyadic (power-of-two) ranges 
 * corresponding to an arbitrary range.  
 * E.g. 14-48 becomes [[14-15], [16-31], [32-47], [48-48]]
 */
typedef struct {
    int64 spans[LONGBITS][2]; /*! the ranges */
    int emptyoffset;        /*! offset of next empty span */
} rangelist;


/*!
 * offset/count pairs for MFV sketches
 */
typedef struct {
    int offset;  /*! memory offset to the value */
    int64 cnt;   /*! counter */
} offsetcnt;


/*!
 * the transition value struct for the mfvsketch aggregate.  Holds a single
 * countmin sketch (no dyadic ranges) and an array of Most Frequent Values.
 * We are flexible with the number of mfvs, as well as the type.
 * Hence at the end of this struct is an array mfv[num_mfvs] of offsetcnt entries,
 * followed by an array of Postgres text objects with the output formats of
 * the mfvs.
 * Each mfv entry contains an offset from the top of the structure where
 * we can find a Postgres text object holding the output format of a
 * frequent value.
 */
typedef struct {
  int    num_mfvs;
  int    next_mfv;
  int    next_offset;
  Oid    typOid;
  Oid    outFuncOid;
  countmin sketch;  /*! a single countmin sketch */
  /*! 
   * type-independent collection of Most Frequent Values
   * Holds an array of (counter,offset) pairs, which by
   * convention is followed by the values themselves as text Datums, 
   * accessible via the offsets
   */
  offsetcnt mfvs[0];
} mfvtransval;

/*! base size of an MFV transval */
#define MFV_TRANSVAL_SZ(i) (VARHDRSZ + sizeof(mfvtransval) + i*sizeof(offsetcnt))

/*! free space remaining for text values */
#define MFV_TRANSVAL_CAPACITY(transblob) (VARSIZE(transblob) - VARHDRSZ - ((mfvtransval *)VARDATA(transblob))->next_offset)

/*! macro to get the text value associated with the i'th mfv */
#define mfv_transval_getval(tvp, i) ((bytea *)(((char*)tvp) + tvp->mfvs[i].offset))


/* countmin aggregate protos */
char *countmin_trans_c(countmin, Datum, Oid);
bytea *cmsketch_check_transval(bytea *);
void countmin_dyadic_trans_c(cmtransval *, int64);

/* countmin scalar function protos */
int64 cmsketch_getcount_c(countmin, Datum, Oid);
Datum cmsketch_rangecount_c(cmtransval *, Datum, Datum);
Datum cmsketch_centile_c(cmtransval *, int, int64);
Datum cmsketch_width_histogram_c(cmtransval *, int64, int64, int);
Datum cmsketch_depth_histogram_c(cmtransval *, int);
void find_ranges(int64, int64, rangelist *);
void find_ranges_internal(int64, int64, int, rangelist *);
// Datum countmin_dump_c(int64 *);

/* hash_counters_iterate and its lambdas */
int64 hash_counters_iterate(Datum, countmin, int64, int64 (*lambdaptr)(
                                uint32,
                                uint32,
                                countmin,
                                int64));

int64 increment_counter(uint32, uint32, countmin, int64);
int64 min_counter(uint32, uint32, countmin, int64);

// bool gt(int64, int64);
// bool eq(int64, int64);
// bool false_fn(int64, int64);
// int64 cmsketch_count_search(cmtransval *, bool (*) (int64, int64), int64, 
                            // bool ( *) (int64, int64), int64);


/* MFV protos */
// Datum cmsketch_mfv_trans_c(int64 *, char *);
bytea *mfv_transval_insert(bytea *, bytea *);
bytea *mfv_transval_replace(bytea *, bytea *, int);
bytea *mfv_transval_insert_at(bytea *, bytea *, int);
// Datum mfvsketch_out_c(mfvtransval *);
int cnt_cmp_desc(const void *i, const void *j);

                            
/* UDF protos */
Datum cmsketch_trans(PG_FUNCTION_ARGS);
Datum cmsketch_getcount(PG_FUNCTION_ARGS);
Datum cmsketch_rangecount(PG_FUNCTION_ARGS);
Datum cmsketch_centile(PG_FUNCTION_ARGS);
Datum cmsketch_width_histogram(PG_FUNCTION_ARGS);
Datum cmsketch_depth_histogram(PG_FUNCTION_ARGS);
Datum cmsketch_out(PG_FUNCTION_ARGS);
Datum cmsketch_combine(PG_FUNCTION_ARGS);
Datum cmsketch_dump(PG_FUNCTION_ARGS);

Datum mfvsketch_trans(PG_FUNCTION_ARGS);
Datum mfvsketch_out(PG_FUNCTION_ARGS);
Datum mfvsketch_array_out(PG_FUNCTION_ARGS);
Datum mfvsketch_combine(PG_FUNCTION_ARGS);

#endif /* _COUNTMIN_H_ */


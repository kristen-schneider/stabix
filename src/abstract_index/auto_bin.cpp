/*
 * Is there an algorithm that can define perfect spaced bin boundaries such that
 * each bin is roughly equally sized? Efficiently? Infers distribution for new
 * data?
 *
 * Repeated Median:
 *    Quick Select (Median Finder):
 *        1. Quicksort-like partitioning based on random pivot.
 *        2. Recurse on larger partition.
 *        O(n) time because region sizes: n + n/2 + n/4...
 *
 *    1. Apply quick select O(n)
 *    2. Partition based on median
 *    3. Recurse on each section until bin count is reached O(log(b)) times
 *    O(n log(b)) time
 *
 *    Further Optimization:
 *        Quick select partitions should be reusable
 *        during Repeated Median partioning.
 */

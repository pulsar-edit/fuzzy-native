
/**
 * The options that can be passed to {@link Matcher#match}.
 */
export type MatcherOptions = {
  /** Whether matching is case-sensitive. Defaults to `false`. */
  caseSensitive?: boolean;

  /** How many results to return at the maximum. Defaults to no limit. */
  maxResults?: number;

  /**
   * Maximum “gap” to allow between consecutive letters for a match candiate.
   * Provide a smaller value to speed up query results. Defaults to no limit.
   */
  maxGap?: number;

  /**
   * How many threads to use while searching. Defaults to `1`.
   */
  numThreads?: number;

  /**
   * Whether to return metadata about the indices of the characters that
   * matched in each returned max. Defaults to `false`.
   */
  recordMatchIndexes?: boolean;

  /**
   * The algorithm to use for fuzzy-matching. If `"fuzzaldrin"`, will use that
   * algorithm for legacy support. Any other value, including the default of
   * `undefined`, will trigger use of the default algorithm.
   */
  algorithm?: 'fuzzaldrin' | undefined;
};

/**
 * A single result returned by {@link Matcher#match}.
 */
export type MatchResult = {
  /** A unique identifier for the match. */
  id: number;

  /** The string value of the match. */
  value: string;

  /**
   * A number in the range (0, 1] — i.e., the maximum value is `1` and the
   * minimum value is the smallest possible positive value. Higher scores mean
   * more relevant matches. `0` means “no match” and will never be returned.
   */
  score: number;

  /**
   * Matching charcter index in `value` for each character in `query`. This can
   * be costly, so this information is returned only when
   * {@link MatcherOptions.recordMatchIndexes} is `true`.
   */
  matchIndexes?: number[];
}

export class Matcher {
  /**
   * Construct a new {@link Matcher} object.
   *
   * You may specify candidates at instantiation time (with the same arguments
   * used by {@link addCandidates} and {@link setCandidates}) or you may wait
   * and add candidates later.
   *
   * @param ids A list of numeric IDs. Must correspond to the candidates
   *  themselves.
   * @param candidates A list of candidates against which we will be matching.
   */
  constructor();
  constructor(ids: number[], candidates: string[]);

  /**
   * Find all candidates that match the given query.
   *
   * @param query The input against which candidates will be searched.
   * @param options Any {@link MatcherOptions}.
   */
  match(query: string, options?: MatcherOptions): MatchResult[];

  /**
   * Add candidates to the list.
   *
   * You are responsible for ensuring that the IDs you use do not match the IDs
   * of any candidates that are already present in the `Matcher`. Any
   * candidates whose IDs already exist in the `Matcher` are silently ignored.
   *
   * @param ids A list of numeric IDs. Must correspond to the candidates
   *  themselves.
   * @param candidates A list of candidates against which we will be matching.
   */
  addCandidates(ids: number[], candidates: string[]): void;

  /**
   * Remove candidates from the list.
   *
   * @param ids The unique identifiers for each of the candidates you want to
   *  remove. Must be an array; if you want to remove only one candidate, wrap
   *  the value in an array first.
   */
  removeCandidates(ids: number[]): void;

  /**
   * Set a complete list of candidates, removing any candidate that may already
   * be defined.
   *
   * If you want to add candidates instead without removing any that may
   * already exist, use {@link addCandidates}.
   *
   * @param ids A list of numeric IDs. Must correspond to the candidates
   *  themselves.
   * @param candidates A list of candidates against which we will be matching.
   */
  setCandidates(ids: number[], candidates: string[]): void;
}

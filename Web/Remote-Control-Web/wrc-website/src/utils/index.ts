import { InternalError } from "../constant";

export function withTimeout<T>(p: Promise<T>, timeout: number): Promise<T> {
  const setTimeoutPromise: Promise<T> = new Promise((_, reject) => {
    window.setTimeout(() => reject(InternalError.TIMEOUT), timeout);
  });

  return Promise.race([p, setTimeoutPromise]);
}
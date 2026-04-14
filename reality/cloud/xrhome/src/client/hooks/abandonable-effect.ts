import {useEffect} from 'react'

// An Abandonable function returns a promise or is a promise itself.
type Abandonable<T> = (() => Promise<T>) | Promise<T>

// The executor of abandonable functions may or may not resolve.
// If it does resolve, it returns whatever the given returned.
type AbandonableExecutor = <T>(abandonable: Abandonable<T>) => Promise<T | never>

// This is the function you make and pass into useAbandonable()
type AbandonableEffect = (executor: AbandonableExecutor) => (void | Promise<void>)

/**
 *
 * Sample usage:
 *
 * useAbandonable((maybeAbandon) => {
 *   try {
 *     const res = await maybeAbandon(fetchSomeData())
 *
 *     // If we res resolves, the component was not canceled/unmounted.
 *     doSomethingWithRes(res)
 *
 *     await maybeAbandon(doMoreThings())
 *
 *      // If component unmounts by the time doMoreThings() resolves,
 *      // this whole function is abandoned and the next line never runs.
 *
 *      console.log('I don't print if component was unmounted')
 *
 *   } catch (err) {
 *
 *     // If catch an error, the component was not canceled/unmounted.
 *     displayError(err)
 *   }
 * }, [deps])
 *
 * @param effectBodyFn A standard effect that takes a maybeAbandon() function. Use it when you
 * use await... e.g. const res = await maybeAbandon(fetchDataFromServer())
 * @param deps Deps passed into underlying useEffect() call.
 */

const useAbandonableEffect = (effectBodyFn: AbandonableEffect, deps?: any[]) => {
  useEffect(() => {
    let canceled = false

    // !! Don't forget to await the executor's promise in your code otherwise things won't work !!
    const executor: AbandonableExecutor = possiblyAbandon => new Promise((resolve, reject) => {
      // Early exit, don't even execute the given function.
      if (canceled) {
        return
      }

      let promise
      if (typeof possiblyAbandon === 'function') {
        promise = possiblyAbandon()
      } else {
        promise = possiblyAbandon
      }

      Promise.resolve(promise).then((res) => {
        if (canceled) {
          return  // Abandon the promise.
        }
        resolve(res)  // Propagate the resolution.
      }).catch((err) => {
        if (canceled) {
          // eslint-disable-next-line no-console
          console.error(err)

          return  // Abandon the promise.
        }
        reject(err)  // Propagate the rejection.
      })
    })

    // Runs your function.
    effectBodyFn(executor)

    return () => {
      canceled = true
    }
  }, deps)
}

export {
  useAbandonableEffect,
  type AbandonableExecutor,
}

import chai from 'chai'
import chaiAsPromised from 'chai-as-promised'
import vm from 'vm'

import {makeRunQueue} from '../src/shared/run-queue'

chai.should()
chai.use(chaiAsPromised)

const {assert, expect} = chai

const wait = (amount: number) => new Promise(resolve => setTimeout(resolve, amount * 10))

describe('RunQueue Validation Test', () => {
  it('should have the right duration', async () => {
    const TRIES = 40
    const TIME = 0.5
    const B_NUM = -3

    const promiseMutex = makeRunQueue()

    let testNum = 0
    let called = 0

    let previousNumberA = -1

    // this is a longer running function, TIME ms
    const functionA = () => {
      // this invocation will get a unique number
      const ourNumber = process.hrtime()[1]

      promiseMutex.next(() => new Promise((resolve) => {
        // ensure we aren't running right after the previous functionA call
        assert.notEqual(testNum, previousNumberA)
        assert.notEqual(ourNumber, previousNumberA)

        testNum = ourNumber
        previousNumberA = ourNumber

        setTimeout(resolve, TIME)
      }).then(() => {
        // after waiting, make sure nothing changed our value
        assert.equal(testNum, ourNumber)
        called++
      }))
    }

    // this function returns quick, we want it to be interlaced beween funcationA calls
    const functionB = () => {
      promiseMutex.next(() => {
        // ensure we weren't the previously ran function
        assert.notEqual(testNum, B_NUM)
        testNum = B_NUM
        called++
      })
    }

    const begin = process.hrtime()

    await new Promise(async (allDone) => {
      for (let i = 0; i < TRIES; i++) {
        // launch both functions async
        functionA()
        functionB()
      }
      promiseMutex.next(allDone)
    })

    // assert.equal(called, TRIES * 2)

    // ensure that the tasks indeed ran synchronously
    const deltaTime = process.hrtime(begin)
    const deltaMillis = deltaTime[0] * 1000 + deltaTime[1] / 1e6
    expect(deltaMillis).to.be.at.least(TRIES * TIME)

    const res = await promiseMutex.next(() => 9001)
    expect(res).to.be.equal(9001)
  }).timeout(10 * 1000)

  it('should not swallow up rejections mid-chain', (done) => {
    const promiseMutex = makeRunQueue()

    const rejectedPromise1 = promiseMutex.next(() => new Promise((resolve, reject) => setTimeout(() => {
      reject(new Error('f'))
    }), 15))

    assert.isRejected(rejectedPromise1)

    const resolved1 = promiseMutex.next(() => 9)
    assert.isFulfilled(resolved1)

    promiseMutex.next(() => 5)
    promiseMutex.next(done)
  })

  it('should not swallow up errors mid-chain', (done) => {
    const promiseMutex = makeRunQueue()

    const resolved1 = promiseMutex.next(() => 9)
    assert.isFulfilled(resolved1)

    const rejectPromise2 = promiseMutex.next(() => {
      throw new Error('whaooo')
    })

    const rejectPromise3 = promiseMutex.next(async () => {
      await wait(1)
      return 9
    })
    const rejectPromise4 = promiseMutex.next(() => 7)

    assert.isRejected(rejectPromise2)
    assert.becomes(rejectPromise3, 9)
    assert.becomes(rejectPromise4, 7)
    promiseMutex.next(done)
  })

  it('should be able to do basic sequencing', (done) => {
    const queue = makeRunQueue()
    let call = 0
    queue.next(async () => {
      await wait(1)
      assert.equal(call, 0)
      call = 1
    })
    queue.next(async () => {
      await wait(1)
      assert.equal(call, 1)
      call = 2
    })
    queue.next(async () => {
      await wait(1)
      assert.equal(call, 2)
    })
    queue.next(done)
  }).timeout(10 * 1000)

  it('should wait properly', (done) => {
    const queue = makeRunQueue()
    let call = 0
    queue.next(async () => {
      await wait(1)
      assert(call === 0)
      call = 1
    }).then(() => {
      assert(call === 1)
    }).then(done)
  })

  it('should basic throw', async () => {
    const queue = makeRunQueue()

    let thenExecutedAfterError = false
    let caughtCorrectError = false

    await queue.next(async () => {
      await wait(1)
      throw new Error('TEST')
    }).then(() => {
      thenExecutedAfterError = true
    }).catch((e) => {
      if (e.message === 'TEST') {
        caughtCorrectError = true
      }
    })

    assert.isFalse(thenExecutedAfterError)
    assert.isTrue(caughtCorrectError)

    const stillRan = await queue.next(() => 34)
    assert.equal(stillRan, 34)
  })

  it('should not swallow unhandled promise rejection', async () => {
    const queue = makeRunQueue()
    let unhandledPromiseRejectionOccurred = false

    // Looks like we can only catch this when the script is run in a different context
    // this test wants to ensure that the internal splitting of the promise chain for the queue
    // does not swallow up the rejection on the returned chain so that if we don't catch it
    // it will surface
    process.on('unhandledRejection', () => {
      unhandledPromiseRejectionOccurred = true
    })

    const nested = () => {
      queue.next(() => new Promise((resolve, reject) => {
        throw new Error('please don\'t swallow me')
      }))
    }

    const script = new vm.Script('nested()')
    script.runInNewContext({nested})

    let nextBitOfWork = false

    await queue.next(() => {
      nextBitOfWork = true
    })

    await wait(1)

    assert.isTrue(nextBitOfWork)
    assert.isTrue(unhandledPromiseRejectionOccurred, 'making sure promise rejection was unhandled')
  })

  it('should return the result of the task', async () => {
    const queue = makeRunQueue()

    const testValue = await queue.next(() => 55)

    assert.equal(testValue, 55)
  })

  it('should allow multiple parallel requests if requested', async () => {
    const queue = makeRunQueue(2)

    let firstEnded = false
    let firstWasAlreadyEnded = false
    await Promise.all([
      queue.next(async () => {
        await wait(2)
        firstEnded = true
      }),
      queue.next(async () => {
        firstWasAlreadyEnded = firstEnded
        await wait(2)
      }),
    ])

    assert.isFalse(firstWasAlreadyEnded)
  })

  it('should queue and start multiple tasks in the correct order', async () => {
    const queue = makeRunQueue(2)

    let firstEnded = false
    let firstEndedBeforeSecondStarted = false
    let secondEnded = false
    let firstEndedBeforeThirdStarted = false
    let secondEndedBeforeThirdStarted = false
    let firstEndedBeforeFourthStarted = false
    let secondEndedBeforeFourthStarted = false
    let thirdStarted = false
    let thirdStartedBeforeFourthStarted = false

    await Promise.all([
      queue.next(async () => {
        await wait(0.75)
        firstEnded = true
      }),
      queue.next(async () => {
        firstEndedBeforeSecondStarted = firstEnded
        await wait(0.5)
        secondEnded = true
      }),
      queue.next(async () => {
        firstEndedBeforeThirdStarted = firstEnded
        secondEndedBeforeThirdStarted = secondEnded
        thirdStarted = true
        await wait(0.5)
      }),
      queue.next(async () => {
        firstEndedBeforeFourthStarted = firstEnded
        secondEndedBeforeFourthStarted = secondEnded
        thirdStartedBeforeFourthStarted = thirdStarted
        await wait(0.5)
      }),
    ])

    assert.isFalse(firstEndedBeforeSecondStarted)
    assert.isFalse(firstEndedBeforeThirdStarted)
    assert.isTrue(secondEndedBeforeThirdStarted)
    assert.isTrue(firstEndedBeforeFourthStarted)
    assert.isTrue(secondEndedBeforeFourthStarted)
    assert.isTrue(thirdStartedBeforeFourthStarted)
  })
})

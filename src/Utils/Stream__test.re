module Js' = Js;
open Relude
open TestFramework

describe("Stream.make, Stream.subscribe", ({ testAsync }) => {
  testAsync("should create a stream on numbers and subscribe to it", ({ expect, callback }) => {
    let stream = Stream.make((~next, ~complete, ~cancel) => {
      next(5);
      next(25);
      next(500);
      complete("end");
      None;
    });

    let _ = stream |> Stream.subscribe(fun
      | Complete(x) => {
        expect.string(x).toEqual("end");
        callback() |> ignore;
      }
      | Next(x) =>
        expect.bool([|5, 25, 500|] |> Array.containsBy(Int.eq, x)).toBeTrue()
      | _ => ()
    );
  });

  testAsync("should stop accepting values after complete", ({ expect, callback }) => {
    let stream = Stream.make((~next, ~complete, ~cancel) => {
      next(5);
      next(25);
      complete("end");
      next(500);
      None;
    });

    let _ = stream |> Stream.subscribe(fun
      | Complete(x) => {
        expect.string(x).toEqual("end");
        callback() |> ignore;
      }
      | Next(x) =>
        expect.bool([|5, 25|] |> Array.containsBy(Int.eq, x)).toBeTrue()
      | _ => ()
    );
  });

  testAsync("should cleanup on cancel from inside", ({ expect, callback }) => {
    let mockCleanupFunction = Mock.fn();
    let stream = Stream.make((~next, ~complete, ~cancel) => {
      Js'.Global.setTimeout(cancel, 0) |> ignore;
      Some(mockCleanupFunction);
    });

    let _ = stream |> Stream.subscribe(fun
      | Cancelled => {
        expect.value(mockCleanupFunction |> Mock.calls).toEqual([|
          [|Js'.undefined|]
        |]);
        callback();
      }
      | _ => ()
    );
  });

  testAsync("should cleanup on cancel from outside", ({ expect, callback }) => {
    let mockCleanupFunction = Mock.fn();
    let stream = Stream.make((~next, ~complete, ~cancel) => {
      Some(mockCleanupFunction);
    });

    let cleanup = stream |> Stream.subscribe(fun
      | Cancelled => callback()
      | _ => ()
    );

    cleanup();
    expect.value(mockCleanupFunction |> Mock.calls).toEqual([|
      [|Js'.undefined|]
    |]);
  });
});


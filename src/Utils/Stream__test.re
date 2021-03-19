open Relude
open TestFramework

describe("Stream.make, Stream.subscribe", ({ testAsync }) => {
  testAsync("should create a stream on numbers and subscribe to it", ({ expect, callback }) => {
    let stream = Stream.make((~next, ~complete, ~cancel) => {
      next(5);
      next(25);
      next(500);
      complete(3);
      None;
    });

    let _ = stream |> Stream.subscribe(fun
      | Complete(x) => {
        expect.int(x).toBe(3);
        callback() |> ignore;
      }
      | Next(x) =>
        expect.bool([|5, 25, 500|] |> Array.containsBy(Int.eq, x)).toBeTrue()
      | _ => ()
    );
  });
});


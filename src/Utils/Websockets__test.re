open Relude.Globals;
open TestFramework;
open Websockets;

let onSendData: int => unit = Mock.fn();

let mockWs = 46;

module WS = {
  type t = int;
  type rxData = int;
  type txData = int;
  type error = string;

  let maxAutoRetries = 2;

  let createConnection = IO.async(resolve => resolve(Ok(mockWs)));

  let sendData = (_, data) => IO.async(resolve => {
    onSendData(data)->ignore;
    () |> Result.ok |> resolve;
  });

  let getDataStream = _ => Stream.make((~next, ~complete as _, ~cancel as _) => {
    next(20);
    next(30);
    next(40);
    next(50);
    None;
  });
};

module Machine = Make(WS);

describe("Websocket happy case", ({ testAsync }) => {
  testAsync("should create a connection and open after resolving", ({ expect, callback }) => {
    let transition = Idle->Machine.transition(Connect);
    let state = transition |> Machine.getStateFromTransition;
    expect.value(state).toEqual(Connecting);

    transition |> Machine.execute(action => {
      expect.value(action).toEqual(Some(OnConnectionOpen(mockWs)));
      callback();
    }) |> ignore
  });

  testAsync("should send data correctly over ws", ({ expect, callback }) => {
    let transition = Connected(mockWs)->Machine.transition(SendData(69));
    let state = transition |> Machine.getStateFromTransition;
    expect.value(state).toEqual(Connected(mockWs));

    transition |> Machine.execute(action => {
      expect.value(action).toEqual(None);
      callback();
    }) |> ignore
  });

  testAsync("should trigger OnData for every message event", ({ expect, callback }) => {
    let transition = Connecting->Machine.transition(OnConnectionOpen(mockWs));
    let state = transition |> Machine.getStateFromTransition;
    expect.value(state).toEqual(Connected(mockWs));

    let data = ref([||]);
    transition |> Machine.execute(action => {
      let n = switch action { | Some(OnData(d)) => d | _ => -1 };
      data.contents = Array.concat(data.contents, [|n|]);
      if (n === 50) {
        expect.value(data.contents).toEqual([| 20, 30, 40, 50 |])
        callback()
      }
    }) |> ignore
  });
});

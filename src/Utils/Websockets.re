open Relude;
open CoreUtils;
open FSM;

let sendWsData
: BsWebSocket.t => string => Effect.IOEff.t('string)
= (ws, data) => IO.async(resolve => {
  ws->BsWebSocket.send(data);
  () |> Result.ok |> resolve;
});

type data = string;
type error = string;
type retryCount = int;

[@bs.deriving accessors]
type state' =
  | Idle
  | Connecting
  | Connected(BsWebSocket.t)
  | ConnectedRxData(BsWebSocket.t, data)
  | AttemptingRetry(retryCount)
  | ConnectionFailed(retryCount)
  | Disconnected
  | Failure(error);

[@bs.deriving accessors]
type msg' =
  | Connect
  | OnConnectionOpen(BsWebSocket.t)
  | OnData(data)
  | SendData(data)
  | OnFailure(error)
  | Retry;

let createConnection = IO.async(resolve => {
  open BsWebSocket;
  let ws = make("wss://echo.websocket.org");
  ws->onError(resolve << Result.error << const("fuck"));
  ws->onClose(resolve << Result.error << const("disconnected"));
  ws->onOpen(resolve << Result.ok << const(ws));
});

let wsDataStream = ws => Stream.make((~next, ~complete as _, ~cancel) => {
  ws->BsWebSocket.onMessage(next << BsWebSocket.MessageEvent.data)
  ws->BsWebSocket.onError(_ => cancel());
  ws->BsWebSocket.onClose(_ => cancel());
  Some(() => BsWebSocket.close(ws))
});

// IO ?(OnConnectionOpen w | Error e)
let connect = createConnection |> IO.mapHandleError(Option.some << onConnectionOpen, Option.some << onFailure);

// IO ?(OnConnectionOpen w | Retry)
let retryConnection = createConnection |> IO.mapHandleError(Option.some << onConnectionOpen, const(Some(Retry)))

// IO ?(OnConnectionOpen w | Retry)
let retryConnectionManual = createConnection |> IO.mapHandleError(Option.some << onConnectionOpen, const(None))

// Stream ?(OnData m)
let streamData = ws => wsDataStream(ws) |> Stream.map(Option.some << onData)

module WSStateChart = {
  type state = state';
  type msg = msg';

  let maxRetries = 3;

  let update = fun
    // Happy connection
    | (Idle | AttemptingRetry(_), Connect) => EffIO(Connecting, connect)
    | (Connecting | AttemptingRetry(_), OnConnectionOpen(ws)) => EffStream(Connected(ws), streamData(ws))
    | (Connected(ws) | ConnectedRxData(ws, _), OnData(data)) => Pure(ConnectedRxData(ws, data))

    // Data
    | ((Connected(ws) | ConnectedRxData(ws, _)) as s, SendData(data)) => EffIO(s, sendWsData(ws, data) |> IO.map(const(None)))

    // On failure
    | (Connected(_) | ConnectedRxData(_) | Connecting, OnFailure(_)) => EffIO(ConnectionFailed(maxRetries), retryConnection)
    | (AttemptingRetry(n), OnFailure(_)) => EffIO(ConnectionFailed(n - 1), retryConnection)

    // Auto and manual retrying
    | (ConnectionFailed(n), Retry) when n > 0 => EffIO(AttemptingRetry(n - 1), retryConnection)
    | (ConnectionFailed(_), Retry) => Pure(Disconnected)
    | (Disconnected, Retry) => EffIO(AttemptingRetry(0), retryConnectionManual)
    | (s, _) => Pure(s)
};

module Machine = MakeStateMachine(WSStateChart);

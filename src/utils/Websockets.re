open Relude.Globals;
/*open CoreUtils;*/
open FSM;

type retryCount = int;

[@bs.deriving accessors]
type stateT('ws, 'rx, 'error) =
  | Idle
  | Connecting
  | Connected('ws)
  | ConnectedRxData('ws, 'rx)
  | AttemptingRetry(retryCount)
  | ConnectionFailed(retryCount)
  | Disconnected
  | Failure('error);

let showState = fun
  | Idle => "Idle"
  | Connecting => "Connecting"
  | Connected(_) => "Connected(<conn>)"
  | ConnectedRxData(_, data) => "ConnectedRxData(<conn>, " ++ data ++ ")"
  | AttemptingRetry(retryCount) => "AttemptingRetry(" ++ Int.toString(retryCount) ++ ")"
  | ConnectionFailed(retryCount) => "ConnectionFailed(" ++ Int.toString(retryCount) ++ ")"
  | Disconnected => "Disconnected"
  | Failure(error) => "Failure(" ++ error ++ ")";

[@bs.deriving accessors]
type msgT('ws, 'rx, 'tx, 'error) =
  | Connect
  | OnConnectionOpen('ws)
  | OnData('rx)
  | SendData('tx)
  | OnFailure('error)
  | Retry;

module type WebsocketApi = {
  type t
  type rxData
  type txData
  type error

  let maxAutoRetries: int;
  let createConnection: IO.t(t, error);
  let sendData: t => txData => Effect.IOEff.t(unit);
  let getDataStream: t => Effect.StreamEff.t(rxData);
};

module WSStateChart = (WS: WebsocketApi) => {
  type state = stateT(WS.t, WS.rxData, WS.error);
  type msg = msgT(WS.t, WS.rxData, WS.txData, WS.error);

  let connIO = WS.createConnection;

  // IO ?(OnConnectionOpen w | Error e)
  let connect = connIO |> IO.mapHandleError(Option.some << onConnectionOpen, Option.some << onFailure);

  // IO ?(OnConnectionOpen w | Retry)
  let retryConnection = connIO |> IO.mapHandleError(Option.some << onConnectionOpen, const(Some(Retry)))

  // IO ?(OnConnectionOpen w)
  let retryConnectionManual = connIO |> IO.mapHandleError(Option.some << onConnectionOpen, const(None))

  // WS.t => Stream ?(OnData m)
  let streamData
    : WS.t => Effect.StreamEff.t(option(msg))
    = Stream.map(Option.some << onData) << WS.getDataStream;

  let update
    : ((state, msg)) => transitionType(state, msg)
    = fun
    // Happy connection
    | (Idle | AttemptingRetry(_), Connect) => EffIO(Connecting, connect)
    | (Connecting | AttemptingRetry(_), OnConnectionOpen(ws)) => EffStream(Connected(ws), streamData(ws))
    | (Connected(ws) | ConnectedRxData(ws, _), OnData(data)) => Pure(ConnectedRxData(ws, data))

    // Data
    | ((Connected(ws) | ConnectedRxData(ws, _)) as s, SendData(data)) =>
        EffIO(s, WS.sendData(ws, data) |> IO.map(const(None)))

    // On failure
    | (Connected(_) | ConnectedRxData(_) | Connecting, OnFailure(_)) =>
        EffIO(ConnectionFailed(WS.maxAutoRetries), retryConnection)
    | (AttemptingRetry(n), OnFailure(_)) => EffIO(ConnectionFailed(n - 1), retryConnection)

    // Auto and manual retrying
    | (ConnectionFailed(n), Retry) when n > 0 => EffIO(AttemptingRetry(n - 1), retryConnection)
    | (ConnectionFailed(_), Retry) => Pure(Disconnected)
    | (Disconnected, Retry) => EffIO(AttemptingRetry(0), retryConnectionManual)
    | (s, _) => Pure(s)
};

module Make = (WS: WebsocketApi) => MakeStateMachine(WSStateChart(WS));


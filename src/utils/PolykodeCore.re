open Relude.Globals;
open CoreUtils;

module WS = {
  open BsWebSocket;

  type t = BsWebSocket.t;
  type rxData = string;
  type txData = string;
  type error = string;

  let maxAutoRetries = 3;

  let createConnection = IO.async(resolve => {
    let ws = make("ws://127.0.0.1:3005");
    ws->onError(resolve << Result.error << const("something went wrong"));
    ws->onOpen(resolve << Result.ok << const(ws));
  });

  let sendData = (ws, data) => IO.async(resolve => {
    ws->send(data |> debug(">> sending datra"));
    () |> Result.ok |> resolve;
  });

  let getDataStream = ws => Stream.make((~next, ~complete as _, ~cancel) => {
    ws->BsWebSocket.onMessage(next << BsWebSocket.MessageEvent.data)
    ws->BsWebSocket.onError(_ => cancel());
    ws->BsWebSocket.onClose(_ => cancel());
    None;
    /*Some(() => BsWebSocket.close(ws))*/
  });
};

module Machine = Websockets.Make(WS);


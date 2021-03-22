
module Config = {
  type data = string;
  type error = string;

  let maxAutoRetries = 3;
  let url = "wss://echo.websocket.org";
}

module Machine = Websockets.Make(Config);


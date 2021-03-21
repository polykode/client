open Router;
/*open CoreUtils;*/
/*open UiUtils*/

/*let (getState, dispatch) = Websockets.Machine.make(Idle, dispatch => fun*/
  /*| Websockets.Connected(ws) => {*/
    /*dispatch(Websockets.SendData("$$foobar$$"))*/
    /*Js.log2(">> connection", ws)*/
  /*}*/
  /*| Websockets.ConnectedRxData(_, data) => {*/
    /*Js.log2(">> rxdata", data)*/
  /*}*/
  /*| s => Js.log2(">> state", s)*/
/*);*/

/*dispatch(Websockets.Connect);*/

let make = () => {
  let page = usePage();

  switch page {
  | Home => <HomePage />
  | About => <AboutPage />
  | Editor(id) => <EditPage id={id} />
  | Preview(id) => <PreviewPage id={id} />
  | NotFound => <NotFoundPage />
  }
};


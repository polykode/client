open UiUtils

type state = Idle | Pending | Success(string) | Failure(string)
type msg = Fetch | DoSuccess(string) | DoFailure(string)

let doStuff =
  Relude.IO.pureWithVoid(Some(DoFailure("Hello world")))

let update = fun
| (Idle, Fetch) => Effectful(Pending, doStuff)
| (Pending, DoSuccess(str)) => Pure(Success(str))
| (Pending, DoFailure(err)) => Pure(Failure(err))
| (Failure(_), Fetch) => Effectful(Pending, doStuff)
| (state, _) => Pure(state)

module Test = {
  let init = Pure(Idle);

  [@react.component]
  let make = () => {
    let (state, dispatch) = useMegaReducer(update->pfReducer, init);

    switch state {
    | Idle => (
      <button onClick={_ => dispatch(Fetch)}>
        {text("Click me")}
      </button>
    )
    | Pending => <div>{text("Loading")}</div>
    | Success(str) => <div>{text("Success: " ++ str)}</div>
    | Failure(str) => (
      <div>
        {text("Error: " ++ str)}
        <button onClick={_ => dispatch(Fetch)}>
          {text("Retry")}
        </button>
      </div>
    )
    }
  }
};

ReactDOMRe.renderToElementWithId(<Test />, "root")


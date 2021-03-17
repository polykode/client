open UiUtils

type state = Idle | Pending | Success(string) | Failure(string)
type msg = Fetch | DoSuccess(string) | DoFailure(string)

let doStuff = () =>
  Js.Promise.resolve(Some(DoFailure("Hello world")))

let update = fun
| (Idle, Fetch) => (Pending, doStuff)
| (Pending, DoSuccess(str)) => (Success(str), noop)
| (Pending, DoFailure(err)) => (Failure(err), noop)
| (Failure(_), Fetch) => (Pending, doStuff)
| (state, _) => (state, noop)

module Test = {
  let init = Idle;

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


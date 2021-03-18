open Router
open UiUtils

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


open Router
open UiUtils

let make = () => {
  let page = usePage();

  switch page {
  | Home => <HomePage />
  | About => <AboutPage />
  | Editor(id) => <EditPage />
  | Preview(id) => <PreviewPage />
  | NotFound => <NotFoundPage />
  }
};


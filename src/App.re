open Router
open UiUtils

let make = () => {
  let page = usePage();

  <div>
    <BasicApi />
    {switch page {
    | Home => <HomePage />
    | About => <AboutPage />
    | Editor(id) => <EditPage />
    | Preview(id) => <PreviewPage />
    | NotFound => <NotFoundPage />
    };}
  </div>
};


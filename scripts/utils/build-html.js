const path = require('path');
const fs = require('fs');
const mustache = require('mustache');
const { SOURCEDIR, DISTDIR } = require('./common');

const writeHtml = async ({ scripts = {}, styles = {}, ...data }) => {
  const htmlPath = `${SOURCEDIR}/index.html`;
  const outputPath = `${DISTDIR}/index.html`;

  const stylesStr = Object.entries(styles)
    .map(([f, opts]) => `<link href="${f}" rel="stylesheet" type="text/css" ${opts} />`)
    .join(' ');

  const scriptStr = Object.entries(scripts)
    .map(([f, opts]) => `<script src="${f}" ${opts}></script>`)
    .join(' ');

  const template = await fs.promises.readFile(htmlPath, 'utf-8');
  const contents = mustache.render(template, {
    scripts: scriptStr,
    styles: stylesStr,
    ...data,
  });
  await fs.promises.writeFile(outputPath, contents);
};

module.exports = { writeHtml };


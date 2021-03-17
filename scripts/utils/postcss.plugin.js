const fs = require('fs');
const fse = require('fs-extra');
const postcss = require("postcss");
const util = require('util');
const tmp = require('tmp');
const path = require("path");

const { readFile, writeFile } = fs.promises;
const ensureDir = util.promisify(fse.ensureDir);

module.exports = (options = { plugins: [] }) => ({
  name: "postcss",
  setup: function (build) {
    const { rootDir = options.rootDir || process.cwd() } = options;
    const tmpDirPath = tmp.dirSync().name;
    build.onResolve(
      { filter: /.\.(css)$/, namespace: "file" },
      async (args) => {
        const sourceFullPath = path.resolve(args.resolveDir, args.path);
        const sourceExt = path.extname(sourceFullPath);
        const sourceBaseName = path.basename(sourceFullPath, sourceExt);
        const sourceDir = path.dirname(sourceFullPath);
        const sourceRelDir = path.relative(path.dirname(rootDir), sourceDir);

        const tmpDir = path.resolve(tmpDirPath, sourceRelDir);
        const tmpFilePath = path.resolve(tmpDir, `${sourceBaseName}.css`);
        await ensureDir(tmpDir);

        const css = await readFile(sourceFullPath);

        const env = process.env.NODE_ENV;
        process.env.NODE_ENV = 'production';
        const result = await postcss(options.plugins).process(css, {
          from: sourceFullPath,
          to: tmpFilePath,
        });
        process.env.NODE_ENV = env;

        // Write result file
        await writeFile(tmpFilePath, result.css);

        return { path: tmpFilePath };
      }
    );
  },
});


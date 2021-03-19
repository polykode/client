const transformIgnore = [
  '(@.*/)?(bs-.*|reason-.*)/)',
  'relude',
];

module.exports = {
  moduleDirectories: [ 'node_modules' ],
  rootDir: '.',
  roots: [ 'src' ],
  testMatch: [ '**/*__test.bs.js' ],
  moduleFileExtensions: ['js'],
  testEnvironment: 'jsdom',
  transformIgnorePatterns: [
    `node_modules/(?!(${transformIgnore.join('|')}).+\\.js$`,
  ],
  collectCoverage: false,
  coveragePathIgnorePatterns: ['node_modules', 'scripts'],
};

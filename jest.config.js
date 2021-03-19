module.exports = {
  moduleDirectories: [ 'node_modules' ],
  roots: [ 'src' ],
  testMatch: [ '**/*__test.bs.js' ],
  moduleFileExtensions: ['js'],
  testEnvironment: 'jsdom',
  transformIgnorePatterns: [
    //'node_modules/(?!(bs-platform))',
    '/node_modules/(?!(@.*/)?(bs-.*|reason-.*)/).+\\.js$',
  ],
  collectCoverage: true,
  coveragePathIgnorePatterns: ['node_modules', 'scripts'],
};

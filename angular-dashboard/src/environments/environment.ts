/**
 * @license
 * Copyright Akveo. All Rights Reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 */
// The file contents for the current environment will overwrite these during build.
// The build system defaults to the dev environment which uses `environment.ts`, but if you do
// `ng build --env=prod` then `environment.prod.ts` will be used instead.
// The list of which env maps to which file can be found in `.angular-cli.json`.

export const environment = {
  production: false,

  firebase: {
    apiKey: 'AIzaSyDm_UKlFtJZIxJDhLBCw4ypad-Erefg-Ow',
    // apiKey: '603o4dr3kNDaNIfJotOhbN82cfMGbAOh9nJ21MPh',
    authDomain: 'aulas-sustentables.firebaseapp.com',
    databaseURL: 'https://finalproject-35a1b.firebaseio.com',
    projectId: 'finalproject-35a1b',
    // storageBucket: "gs://finalproject-35a1b.appspot.com",
    // messagingSenderId: "1077643404202"
  }
};


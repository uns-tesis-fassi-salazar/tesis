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
  VAPID_PUBLIC_KEY: 'BOLbda8dOJTcjzsAJd1iF_DFYSHur5e5rerqrSUBdRk9ZeuVxZFVCNk1Wraw3--L1P7X5qaw9-aNoZOVW95Ecco',
  backendURL: 'https://aulas-sustentables.herokuapp.com',
  firebase: {
    apiKey: 'AIzaSyDm_UKlFtJZIxJDhLBCw4ypad-Erefg-Ow',
    authDomain: 'aulas-sustentables.firebaseapp.com',
    databaseURL: 'https://finalproject-35a1b.firebaseio.com',
    projectId: 'finalproject-35a1b',
    storageBucket: "finalproject-35a1b.appspot.com",
    appId: "1:473880504450:web:0e62f92c31ef226337ec9b"
  }
};


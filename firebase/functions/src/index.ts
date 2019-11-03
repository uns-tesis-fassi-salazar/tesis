// // Start writing Firebase Functions
// // https://firebase.google.com/docs/functions/typescript
//
// export const helloWorld = functions.https.onRequest((request, response) => {
//  response.send("Hello from Firebase!");
// });

import * as functions from 'firebase-functions';
import * as telegraf from 'telegraf';
import * as express from 'express';
import * as cors from 'cors';

export const notification = functions.database.instance('finalproject-35a1b').ref('/MovementSensor/value').onUpdate(_ => {
  // const bot = new telegraf.Telegram(functions.config().bot.token);
  const bot = new telegraf.Telegram("980897224:AAGnION1Ii8m0H3du-BC9-4KoRPkW--FcbY");
  return bot.sendMessage(
      // functions.config().bot.chat,
      "-336904340",
      'Cambio el value de MovementSensor en firebase. wiiii'
  );
});


// give us the possibility of manage request properly
const app = express()

// Automatically allow cross-origin requests
app.use(cors({ origin: true }))

// our single entry point for every message
app.post('/', async (req, res) => {
  /*
    You can put the logic you want here
    the message receive will be in this
    https://core.telegram.org/bots/api#update
  */
  const isTelegramMessage = req.body
                          && req.body.message
                          && req.body.message.chat
                          && req.body.message.chat.id
                          && req.body.message.from
                          && req.body.message.from.first_name

  if (isTelegramMessage) {
    const chat_id = req.body.message.chat.id
    // const { first_name } = req.body.message.from

    const reqbody = JSON.stringify(req.body.message)

    return res.status(200).send({
      method: 'sendMessage',
      chat_id,
      // text: `Hello ${first_name}`
      text: reqbody
    })
  }

  return res.status(200).send({ status: 'not a telegram message' })
})
// this is the only function it will be published in firebase
export const router = functions.https.onRequest(app)
import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { AngularFireDatabase } from '@angular/fire/database';
import { SwPush } from '@angular/service-worker';
import { Observable, Subject } from 'rxjs';
import { map, take } from 'rxjs/operators';
import { environment } from '../../environments/environment';
import { UserNotificationSubscription } from '../models/userNotificationSubscrition';
import { AuthService } from './auth.service';

@Injectable({
  providedIn: 'root'
})
export class UserNotificationsService {

  backendURL = environment.backendURL;

  constructor(private http: HttpClient,
    private swPush: SwPush,
    private db: AngularFireDatabase,
    private authService: AuthService,
  ) {

  }

  public getUserNotificationSubscription(uid: string = ''): Observable<UserNotificationSubscription> {
    if (uid === '') {
      const currentUser = this.authService.getCurrentUser();
      uid = currentUser.uid
    }
    return this.db.object<UserNotificationSubscription>('users/notificationsSubscriptions/' + uid)
      .valueChanges();
  }


  async subscribeToNofications() {
    const currentUser = this.authService.getCurrentUser();

    const currentSubscription = await this.swPush.subscription.pipe(take(1)).toPromise();
    console.log('SwPush current subscription: ', currentSubscription);
    if (currentSubscription == null) {
      console.log('Before request subscription')
      let sub;
      let timeout = true;
      try {
        sub = await this.requestSubscription();
      } catch (err) {
        console.log('Error on catch for requestSubscription()', err);
        return Promise.reject(err);
      }

      console.log('Nueva sub: ', sub)
      if (!sub) {
        console.log('error al pedir permisos de notificaciones al dispositivo');
        return Promise.reject('No fue posible subscribirse con el dispositivo actual')
      }

      const subJson = sub.toJSON();
      let userSubscription = new UserNotificationSubscription(currentUser.uid, subJson.endpoint, subJson.expirationTime, subJson.keys);
      console.log('New user subscription: ', userSubscription)
      if (userSubscription.keys && userSubscription.keys.auth && userSubscription.keys.auth !== '') {
        return this.db.object('users/notificationsSubscriptions/' + currentUser.uid + '/devices/' + userSubscription.keys.auth).set(userSubscription);
      }
    }

    return currentSubscription;
  }

  async unsubscribeToNotifications() {
    const currentUser = this.authService.getCurrentUser();
    console.log('Desubscribiendo');
    const currentSubscription = await this.swPush.subscription.pipe(take(1)).toPromise();
    if (currentSubscription != null) {
      const currentSubscriptionJson = currentSubscription.toJSON();
      try {
        await this.db.object('users/notificationsSubscriptions/' + currentUser.uid + '/devices/' + currentSubscriptionJson.keys.auth).remove();
        this.swPush.unsubscribe()
          .then(x => console.log('Desubscripción satisfactoria'))
          .catch(x => console.log('Error al intentar desubscribirse'))
      } catch (e) {
        console.log('Catch error al intentar desubscribirse ')
        return Promise.reject('No fue posible desactivar las notificaciones');
      }
    }
  }

  private async requestSubscription() {
    return this.swPush.requestSubscription({
      serverPublicKey: environment.VAPID_PUBLIC_KEY
    });
  }
}

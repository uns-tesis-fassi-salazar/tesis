import { Component, OnInit } from '@angular/core';
import { SwPush } from '@angular/service-worker';
import { filter, take, takeWhile } from 'rxjs/operators';
import { environment } from '../../../../environments/environment';
import { UserNotificationSubscription } from '../../../models/userNotificationSubscrition';
import { UtilService } from '../../../services';
import { UserNotificationsService } from '../../../services/user-notifications.service';

@Component({
  selector: 'user-notifications',
  templateUrl: './user-notifications.component.html',
  styleUrls: ['./user-notifications.component.scss']
})
export class UserNotificationsComponent implements OnInit {

  private alive = true;

  constructor(
    private userNotificationsService: UserNotificationsService,
    private utilService: UtilService,
    private swPush: SwPush
  ) { }

  ngOnInit() {
  }

  ngOnDestroy() {
    this.alive = false;
  }

  public get notificationsGranted() {
    return Notification.permission === 'granted';
  }

  onNotificationsToggle(newToggleButtonValue) {
    if (newToggleButtonValue) {
      this.userNotificationsService.subscribeToNofications()
        .then(x => this.utilService.showToast('success', 'Notificaciones activadas satisfactoriamente!'))
        .catch(err => {
          console.log(err);
          this.utilService.showError(err, 'No se pudo activar las notificaciones', 'Por favor vuelva a intentarlo', 4000)
        });
    } else {
      this.userNotificationsService.unsubscribeToNotifications()
        .then(x => this.utilService.showToast('success', 'Notificaciones desactivadas satisfactoriamente!'))
        .catch(err => {
          this.utilService.showError(err, 'No se pudo desactivar las notificaciones', 'Por favor vuelva a intentarlo', 4000)
        });
    }
  }

  test() {
    this.swPush.subscription.toPromise()
      .then(sub => console.log('Current sub in test by promise: ', sub ? sub.toJSON() : sub))
      .catch(err => console.log('Error in test by promise: ', err));
  }

}

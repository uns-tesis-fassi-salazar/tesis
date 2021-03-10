import { Component, OnInit } from '@angular/core';
import { SwPush } from '@angular/service-worker';
import { environment } from '../../../../environments/environment';
import { NewsletterService } from '../../../services/newsletter.service';

@Component({
  selector: 'user-notifications',
  templateUrl: './user-notifications.component.html',
  styleUrls: ['./user-notifications.component.scss']
})
export class UserNotificationsComponent implements OnInit {

  constructor(
    private swPush: SwPush,
    private newsletterService: NewsletterService
  ) { }

  ngOnInit() {
  }

  subscribeToNotifications() {
    this.swPush.requestSubscription({
      serverPublicKey: environment.VAPID_PUBLIC_KEY
    })
      .then(sub => {
        console.log(sub);
        this.newsletterService.addPushSubscriber(sub).subscribe();
      })
      .catch(err => console.error("Could not subscribe to notifications", err));
  }

}

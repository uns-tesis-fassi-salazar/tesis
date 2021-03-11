/**
 * @license
 * Copyright Akveo. All Rights Reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 */
import { Component, OnInit } from '@angular/core';
import { SwPush, SwUpdate } from '@angular/service-worker';
import { AnalyticsService } from './@core/utils/analytics.service';
import { UtilService } from './services';
import { SwCheckForUpdateService } from './services/swCheckUpdate.service';
import { UserNotificationsService } from './services/user-notifications.service';

@Component({
  selector: 'ngx-app',
  template: '<router-outlet></router-outlet>',
})
export class AppComponent implements OnInit {
  readonly VAPID_PUBLIC_KEY = "BKqz1XBL9ngQbvRV79l3cuwMA3jkTsLNyTkRThBBTcRXNmKbgfCvTD95cZLBR9KVpdKfTNXh2vl6lhxiUxdwAWM";

  constructor(
    private analytics: AnalyticsService,
    private swPush: SwPush,
    private swCheckForUpdateService: SwCheckForUpdateService,
    private swUpdates: SwUpdate,
    private utilService: UtilService
  ) { 
    this.swPush.notificationClicks.subscribe( event => {
      const url = event.notification.data.url;
      window.open(url, '_blank');
    });

    this.swUpdates.available.subscribe((event) => {
      this.utilService.showToast('primary', '¡Nueva versión de la app disponible!', 'Para actualizarla por favor recargar la página.', 8000);
    });


  }

  ngOnInit(): void {
    this.analytics.trackPageViews();
  }

  
}

/**
 * @license
 * Copyright Akveo. All Rights Reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 */
import { Component, OnInit } from '@angular/core';
import { SwPush } from '@angular/service-worker';
import { AnalyticsService } from './@core/utils/analytics.service';
import { NewsletterService } from './services/newsletter.service';

@Component({
  selector: 'ngx-app',
  template: '<router-outlet></router-outlet>',
})
export class AppComponent implements OnInit {
  readonly VAPID_PUBLIC_KEY = "BKqz1XBL9ngQbvRV79l3cuwMA3jkTsLNyTkRThBBTcRXNmKbgfCvTD95cZLBR9KVpdKfTNXh2vl6lhxiUxdwAWM";

  constructor(
    private analytics: AnalyticsService,
  ) { }

  ngOnInit(): void {
    this.analytics.trackPageViews();
  }

  
}

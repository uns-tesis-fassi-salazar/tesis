import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { environment } from '../../environments/environment';

@Injectable({
  providedIn: 'root'
})
export class NewsletterService {

  backendURL = environment.backendURL;

  constructor(private http: HttpClient) {

  }

  addPushSubscriber(sub:any) {
      return this.http.post(this.backendURL + '/api/notifications', sub);
  }

  send() {
      console.log('Triggering push notification')
      return this.http.post(this.backendURL + '/api/newsletter', null);
  }
}

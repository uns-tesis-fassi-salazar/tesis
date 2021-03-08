import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class NewsletterService {

  constructor(private http: HttpClient) {

  }

  addPushSubscriber(sub:any) {
      return this.http.post('http://localhost:9000/api/notifications', sub);
  }

  send() {
      console.log('Triggering push notification')
      return this.http.post('http://localhost:9000/api/newsletter', null);
  }
}

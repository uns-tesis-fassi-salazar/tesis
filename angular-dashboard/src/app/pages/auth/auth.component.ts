import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'ngx-auth',
  template: `
    <nb-layout center>
      <nb-layout-column>
        <router-outlet></router-outlet>
      </nb-layout-column>
    <nb-layout>
  `
})
export class MyAuthComponent {

  constructor() { }
}

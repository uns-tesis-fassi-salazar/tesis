import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'ngx-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.scss']
})
export class HomeComponent implements OnInit {
  nombreFotos: string[];
  pathToFotos = '../../../assets/images/proyecto/';

  constructor() { }

  ngOnInit() {
    this.nombreFotos = [
      '1.jpeg',
      '2.jpeg',
      '3.jpeg',
      '4.jpg',
      '5.jpeg',
      '6.jpeg',
      '7.jpeg',
      '8.jpeg',
      '9.jpg',
      '10.jpg',
      // '11.jpg',
      // '12.jpg',
    ];
  }

}

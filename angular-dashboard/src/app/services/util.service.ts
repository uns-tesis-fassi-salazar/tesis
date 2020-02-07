import { Injectable } from '@angular/core';
import { AngularFireDatabase, AngularFireList } from '@angular/fire/database';
import { Observable } from 'rxjs';
import { NbToastrService, NbGlobalPosition, NbGlobalPhysicalPosition } from '@nebular/theme';

@Injectable({
  providedIn: 'root'
})
export class UtilService {

  constructor(
    private toastrService: NbToastrService
  ) { }

  showToast(status, title, message = '', duration = 2500, position = NbGlobalPhysicalPosition.BOTTOM_RIGHT ) {
    this.toastrService.show(
      message,
      title,
      { position, status, duration });
  }

}

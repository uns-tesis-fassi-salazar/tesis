import { Component, Input } from '@angular/core';
import { NbDialogRef } from '@nebular/theme';

@Component({
  selector: 'decision-dialog',
  templateUrl: 'decision-dialog.component.html',
  styleUrls: ['decision-dialog.component.scss'],
})
export class DecisionDialogComponent {

  @Input() message: string;

  constructor(protected ref: NbDialogRef<DecisionDialogComponent>) {}

  no() {
    this.ref.close(false);
  }

  yes() {
    this.ref.close(true);
  }
}

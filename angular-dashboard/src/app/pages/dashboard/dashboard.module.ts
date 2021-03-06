import { NgModule } from '@angular/core';
import {
  NbActionsModule,
  NbButtonModule,
  NbCardModule,
  NbTabsetModule,
  NbUserModule,
  NbRadioModule,
  NbSelectModule,
  NbListModule,
  NbIconModule,
} from '@nebular/theme';
import { NgxEchartsModule } from 'ngx-echarts';

import { ThemeModule } from '../../@theme/theme.module';
import { DashboardComponent } from './dashboard.component';
import { StatusCardComponent } from './status-card/status-card.component';
import { TemperatureComponent } from './temperature/temperature.component';
import { TemperatureDraggerComponent } from './temperature/temperature-dragger/temperature-dragger.component';
import { FormsModule } from '@angular/forms';
import { SensorDisplayComponent } from './sensor-display/sensor-display.component';
import { NgbDropdownModule } from '@ng-bootstrap/ng-bootstrap';

@NgModule({
  imports: [
    FormsModule,
    ThemeModule,
    NbCardModule,
    // NbUserModule,
    NbButtonModule,
    NbTabsetModule,
    // NbActionsModule,
    NbRadioModule,
    // NbSelectModule,
    // NbListModule,
    NbIconModule,
    // NgxEchartsModule,
    NgbDropdownModule
  ],
  declarations: [
    DashboardComponent,
    StatusCardComponent,
    TemperatureDraggerComponent,
    TemperatureComponent,
    SensorDisplayComponent,
  ],
})
export class DashboardModule { }

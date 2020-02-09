import { NgModule } from '@angular/core';
import { NbMenuModule, NbAlertModule, NbIconModule, NbSidebarModule, NbSelectModule, NbButtonModule, NbInputModule, NbCardModule, NbDialogModule, NbActionsModule, NbCheckboxModule, NbProgressBarModule, NbListModule } from '@nebular/theme';

import { NgSelectModule } from '@ng-select/ng-select';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { ConfigurationRoutingModule } from './configuration-routing.module';
import { ThemeModule } from '../../@theme/theme.module';
import { MiscellaneousModule } from '../miscellaneous/miscellaneous.module';
import { ConfigurationComponent } from './configuration.component';
import { NgbDropdownModule } from '@ng-bootstrap/ng-bootstrap';
import { ConfigurationFirmwareComponent } from './configuration-firmware/configuration-firmware.component'
import { FilterByPropertyPipe } from '../../utils';
import { ConfigurationIrComponent } from './configuration-ir/configuration-ir.component';

@NgModule({
  imports: [
    ConfigurationRoutingModule,
    ThemeModule,
    MiscellaneousModule,
    NbSidebarModule,
    NbMenuModule,
    NbSelectModule,
    NbIconModule,
    NbButtonModule,
    NbInputModule,
    NbCardModule,
    NgSelectModule,
    NbDialogModule.forChild(),
    NbCheckboxModule,
    NbAlertModule,
    NbProgressBarModule,
    NbListModule,
    
    NgbDropdownModule,

    ReactiveFormsModule,
    FormsModule
  ],
  declarations: [
    ConfigurationComponent,
    ConfigurationFirmwareComponent,
    ConfigurationIrComponent,
  ],
  entryComponents: [
  ]
})
export class ConfigurationModule {
}

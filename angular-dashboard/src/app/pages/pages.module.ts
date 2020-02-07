import { NgModule } from '@angular/core';
import { NbMenuModule, NbAlertModule, NbIconModule, NbSidebarModule, NbSelectModule, NbButtonModule, NbInputModule, NbCardModule, NbDialogModule, NbActionsModule, NbCheckboxModule } from '@nebular/theme';

import { ThemeModule } from '../@theme/theme.module';
import { PagesComponent } from './pages.component';
import { DashboardModule } from './dashboard/dashboard.module';
import { PagesRoutingModule } from './pages-routing.module';
import { MiscellaneousModule } from './miscellaneous/miscellaneous.module';
import { HomeComponent } from './home/home.component';

import { NgSelectModule } from '@ng-select/ng-select';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { ConfigurationModule } from './configuration/configuration.module';
import { DialogNamePromptComponent, FilterByPropertyPipe, DecisionDialogComponent } from '../utils';
import { ListEdificioComponent } from './edificios-aulas/list-edificio/list-edificio.component';
import { CreateEdificioComponent } from './edificios-aulas/create-edificio/create-edificio.component';
import { ListAulaComponent } from './edificios-aulas/list-aula/list-aula.component';
import { EditAulaComponent } from './edificios-aulas/edit-aula/edit-aula.component';
import { EditEdificioComponent } from './edificios-aulas/edit-edificio/edit-edificio.component';
import { CreateAulaComponent } from './edificios-aulas/create-aula/create-aula.component';
import { NgbDropdownModule } from '@ng-bootstrap/ng-bootstrap';

@NgModule({
  imports: [
    PagesRoutingModule,
    ThemeModule,
    DashboardModule,
    ConfigurationModule,
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
    
    NgbDropdownModule,

    ReactiveFormsModule,
    FormsModule
  ],
  declarations: [
    PagesComponent,
    HomeComponent,
    CreateEdificioComponent,
    CreateAulaComponent,
    ListAulaComponent,
    EditAulaComponent,
    EditEdificioComponent,
    ListEdificioComponent,

    FilterByPropertyPipe,
    DialogNamePromptComponent,
    DecisionDialogComponent,
  ],
  entryComponents: [
    DialogNamePromptComponent,
    DecisionDialogComponent,
  ]
})
export class PagesModule {
}

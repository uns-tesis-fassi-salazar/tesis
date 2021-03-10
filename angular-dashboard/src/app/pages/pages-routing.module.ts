import { RouterModule, Routes } from '@angular/router';
import { NgModule } from '@angular/core';

import { PagesComponent } from './pages.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { NotFoundComponent } from './miscellaneous/not-found/not-found.component';
import { HomeComponent } from './home/home.component';
import { ConfigurationComponent } from './configuration/configuration.component';
import { UrlRoutes } from '../helpers';
import { EditEdificioComponent } from './edificios-aulas/edit-edificio/edit-edificio.component';
import { CreateEdificioComponent } from './edificios-aulas/create-edificio/create-edificio.component';
import { ListAulaComponent } from './edificios-aulas/list-aula/list-aula.component';
import { CreateAulaComponent } from './edificios-aulas/create-aula/create-aula.component';
import { EditAulaComponent } from './edificios-aulas/edit-aula/edit-aula.component';
import { ListEdificioComponent } from './edificios-aulas/list-edificio/list-edificio.component';
import { LogsComponent } from './logs/logs.component';
import { ShowLogsComponent } from './logs/show-logs/show-logs.component';
import { UserNotificationsComponent } from './user/user-notifications/user-notifications.component';

const routes: Routes = [{
  path: '',
  component: PagesComponent,
  children: [
    {
      path: 'inicio',
      component: HomeComponent,
    },
    {
      path: UrlRoutes.edificios,
      children: [
        {
          path: UrlRoutes.editarEdificio,
          component: EditEdificioComponent
        },
        {
          path: UrlRoutes.nuevoEdificio,
          component: CreateEdificioComponent
        },
        {
          path: UrlRoutes.nuevaAula,
          component: CreateAulaComponent
        },
        {
          path: UrlRoutes.aulasEdificio,
          children: [
            {
              path: '',
              component: ListAulaComponent,
            },
            {
              path: UrlRoutes.editarAula,
              component: EditAulaComponent
            }
          ]
        },
        {
          path: '',
          component: ListEdificioComponent
        }
      ]
    },
    {
      path: 'dashboard/aula/:aulaKey',
      pathMatch: 'full',
      component: DashboardComponent,
    },
    {
      path: 'configuracion',
      loadChildren: () => import('./configuration/configuration.module')
        .then(m => m.ConfigurationModule),
    },
    {
      path: 'logs',
      pathMatch: 'full',
      component: LogsComponent,
    },
    {
      path: 'logs/:nodoMac',
      pathMatch: 'full',
      component: ShowLogsComponent,
    },
    {
      path: 'user/notificaciones',
      pathMatch: 'full',
      component: UserNotificationsComponent,
    },
    {
      path: '',
      pathMatch: 'full',
      redirectTo: 'inicio',
    },
    {
      path: '**', component: NotFoundComponent
    }
  ],
}];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule],
})
export class PagesRoutingModule {
}

import { Component, OnDestroy, OnInit, OnChanges } from '@angular/core';
import { NbThemeService, NbMenuService } from '@nebular/theme';
import { takeWhile, map } from 'rxjs/operators';
import { SolarData } from '../../@core/data/solar';
import { NodoService } from '../../services/nodo.service';
import { Observable, from } from 'rxjs';
import { AulaService } from '../../services/aula.service';
import { Aula } from '../../models';
import { ActivatedRoute, Router } from '@angular/router';
import { UtilService } from '../../services';
import { DBConstants, UrlRoutes } from '../../helpers';


interface CardSettings {
  title: string;
  iconClass: string;
  type: string;
  status: boolean;
}

@Component({
  selector: 'ngx-dashboard',
  styleUrls: ['./dashboard.component.scss'],
  templateUrl: './dashboard.component.html',
})
export class DashboardComponent implements OnDestroy, OnInit {


  private alive = true;

  lightCard: CardSettings = {
    title: 'Luces',
    iconClass: 'nb-lightbulb',
    type: 'warning',
    status: true,
  };

  AirConditionerCard: CardSettings = {
    title: 'Apagar AC',
    iconClass: 'nb-snowy-circled',
    type: 'primary',
    status: true,
  };

  statusCards: string;

  commonStatusCardsSet: CardSettings[] = [
    this.lightCard,
    this.AirConditionerCard,
  ];

  // statusCardsByThemes: {
  //   default: CardSettings[];
  //   cosmic: CardSettings[];
  //   corporate: CardSettings[];
  //   dark: CardSettings[];
  // } = {
  //     default: this.commonStatusCardsSet,
  //     cosmic: this.commonStatusCardsSet,
  //     corporate: [
  //       {
  //         ...this.lightCard,
  //         type: 'warning',
  //       },
  //       {
  //         ...this.AirConditionerCard,
  //         type: 'primary',
  //       }
  //     ],
  //     dark: this.commonStatusCardsSet,
  //   };

  public luminocidad$: Observable<any>;
  public humedad$: Observable<any>;
  public temperatura$: Observable<any>;
  public movimiento$: Observable<any>;
  public hallValue: number = 0;
  public lightCardClicked = false;
  // public tiempoVacia$: Observable<any>;
  public tiempoEntreLecturas$: Observable<any>;

  public aulaData: Aula;

  constructor(private themeService: NbThemeService,
    private nodoService: NodoService,
    private aulaService: AulaService,
    private router: Router,
    private route: ActivatedRoute,
    private utilService: UtilService
  ) {
    this.aulaData = new Aula();
  }

  ngOnInit() {
    // Obtener aula key de la url y despues buscarla en la db
    this.route.paramMap
      .pipe(
        takeWhile(() => this.alive),
        map(paramMap => paramMap.get('aulaKey'))
      )
      .subscribe(aulaKey => {
        this.aulaService.getAulaByKey(aulaKey).pipe(takeWhile(() => this.alive))
          .subscribe(aula => {
            this.checkAula(aula);
            if (this.aulaData.nodoMac != '' && this.aulaData.comandoIR != aula.comandoIR) {
              this.utilService.showToast('primary','Nuevo comando registrado!','Dirijase a las configuraciones de los comandos infrarrojos para guardar la marca y modelo del AC', 8000);
            }
            this.aulaData = aula;
            this.loadValues();
          })
      })
  }

  ngOnDestroy() {
    this.alive = false;
  }

  get esperandoAula() {
    return this.aulaData.key == null;
  }

  get aulaConNodo() {
    return this.aulaData.nodoMac != '';
  }

  loadValues() {
    if (this.aulaData.nodoMac != null || this.aulaData.nodoMac != '') {
      this.luminocidad$ = this.nodoService.getSensor(this.aulaData.nodoMac, DBConstants.nodoSensorLuminocidad);
      this.humedad$ = this.nodoService.getSensor(this.aulaData.nodoMac, DBConstants.nodoSensorHumedad);
      this.temperatura$ = this.nodoService.getSensor(this.aulaData.nodoMac, DBConstants.nodoSensorTemperatura);
      this.movimiento$ = this.nodoService.getSensor(this.aulaData.nodoMac, DBConstants.nodoSensorMovimiento);
      this.nodoService.getSensor(this.aulaData.nodoMac, DBConstants.nodoSensorHall)
        .pipe(takeWhile(() => this.alive))
        .subscribe(hallValue => {
          console.log(hallValue);
          if (!this.lightCardClicked) {
            if (hallValue > 1.8) {
              this.lightCard.status = true;
            } else {
              this.lightCard.status = false;
            }
          }
        });
    }
  }

  goToEditarAula() {
    this.aulaService.updateCurrentAula(this.aulaData);
    this.router.navigate([UrlRoutes.edificios, UrlRoutes.aulasEdificio, UrlRoutes.editarAula])
  }

  grabarComandoAC() {
    this.nodoService.updateNodoComando(this.aulaData.nodoMac, DBConstants.comandoGrabarIR);
    this.utilService.showToast('primary', 'Grabar nuevo comando', 'Dirija el control remoto hacia el nodo y presione el boton de apagado', 5000);
  }

  onClickLightCard(lightCard) {
    lightCard.status = !lightCard.status;
    this.lightCardClicked = true;
    this.nodoService.updateNodoComando(this.aulaData.nodoMac, lightCard.status ? DBConstants.comandoEncenderLuces : DBConstants.comandoApagarLuces);
    setTimeout(() => {
      this.lightCardClicked = false;
    }, 3000);
  }

  onClickACCard(ACCard) {
    ACCard.status = !ACCard.status;
    this.nodoService.updateNodoComando(this.aulaData.nodoMac, DBConstants.comandoEmitirIR);
    setTimeout(() => {
      ACCard.status = !ACCard.status;
    }, 200);
  }

  private checkAula(aula: Aula) {
    if (aula == null || aula.key == null) {
      this.utilService.showToast('danger', 'Aula no encontrada');
      this.router.navigate([UrlRoutes.home]);
    }
  }
}

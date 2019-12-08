import {Component, OnDestroy, OnInit, OnChanges} from '@angular/core';
import { NbThemeService, NbMenuService } from '@nebular/theme';
import { takeWhile } from 'rxjs/operators' ;
import { SolarData } from '../../@core/data/solar';
import { FiredbService } from '../../services/firedb.service';
import { Observable, from } from 'rxjs';
import { AulaService } from '../../services/aula.service';
import { DBConstants, Aula } from '../../models';


interface CardSettings {
  title: string;
  iconClass: string;
  type: string;
  status: boolean;
  actuadorId: string;
}

@Component({
  selector: 'ngx-dashboard',
  styleUrls: ['./dashboard.component.scss'],
  templateUrl: './dashboard.component.html',
})
export class DashboardComponent implements OnDestroy, OnInit {


  private alive = true;
  solarValue: number;
  lightCard: CardSettings = {
    title: 'Luces',
    iconClass: 'nb-lightbulb',
    type: 'warning',
    status: true,
    actuadorId: DBConstants.actuadorLedId
  };
  AirConditionerCard: CardSettings = {
    title: 'Apagar aire acondicionado',
    iconClass: 'nb-snowy-circled',
    type: 'primary',
    status: true,
    actuadorId: DBConstants.actuadorAireId
  };

  statusCards: string;

  commonStatusCardsSet: CardSettings[] = [
    this.lightCard,
    this.AirConditionerCard,
  ];

  statusCardsByThemes: {
    default: CardSettings[];
    cosmic: CardSettings[];
    corporate: CardSettings[];
    dark: CardSettings[];
  } = {
    default: this.commonStatusCardsSet,
    cosmic: this.commonStatusCardsSet,
    corporate: [
      {
        ...this.lightCard,
        type: 'warning',
      },
      {
        ...this.AirConditionerCard,
        type: 'primary',
      }
    ],
    dark: this.commonStatusCardsSet,
  };

  public luminocidad: Observable<any>;
  public humedad: Observable<any>;
  public temperatura: Observable<any>;
  public movimiento: Observable<any>;
  public tiempoVacia: Observable<any>;
  public tiempoEntreLecturas: Observable<any>;

  public nodoMACValue: String;
  public aulaData: Aula;

  constructor(private themeService: NbThemeService,
              private solarService: SolarData,
              private firedbService: FiredbService,
              private aulaService: AulaService
  ) {

    this.themeService.getJsTheme()
      .pipe(takeWhile(() => this.alive))
      .subscribe(theme => {
        this.statusCards = this.statusCardsByThemes[theme.name];
    });

    this.solarService.getSolarData()
      .pipe(takeWhile(() => this.alive))
      .subscribe((data) => {
        this.solarValue = data;
      });

  }

  ngOnInit() {
    this.aulaService.aulaData.subscribe(aulaObj => {
      this.aulaData = aulaObj;
      this.nodoMACValue = aulaObj.nodoMac;
      this.loadValues();
    });
  }

  ngOnDestroy() {
    this.alive = false;
  }

  loadValues(){
    this.luminocidad = this.firedbService.getSensor(this.nodoMACValue, DBConstants.sensorLuminocidadId);
    this.humedad = this.firedbService.getSensor(this.nodoMACValue, DBConstants.sensorHumedadId);
    this.temperatura = this.firedbService.getSensor(this.nodoMACValue, DBConstants.sensorTemperaturaId);
    this.movimiento = this.firedbService.getSensor(this.nodoMACValue, DBConstants.sensorMovimientoId);
    this.tiempoVacia = this.firedbService.getConfig(this.nodoMACValue, DBConstants.configTiempoVacia);
    this.tiempoEntreLecturas = this.firedbService.getConfig(this.nodoMACValue, DBConstants.configTiempoEntreLectura);
  }

  private auxToggle = false;
  onClickStatusButton(statusCard){
      statusCard.status = !statusCard.status;
      this.auxToggle = !this.auxToggle;
      this.firedbService.updateActuador(this.nodoMACValue, statusCard.actuadorId, this.auxToggle);
      setTimeout(() => {
        statusCard.status = !statusCard.status;
      }, 200);
  }
}

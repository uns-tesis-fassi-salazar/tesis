import {Component, OnDestroy, OnChanges} from '@angular/core';
import { NbThemeService, NbMenuService } from '@nebular/theme';
import { takeWhile } from 'rxjs/operators' ;
import { SolarData } from '../../@core/data/solar';
import { FiredbService } from '../../services/firedb.service';
import { Observable } from 'rxjs';

interface CardSettings {
  title: string;
  iconClass: string;
  type: string;
}

@Component({
  selector: 'ngx-dashboard',
  styleUrls: ['./dashboard.component.scss'],
  templateUrl: './dashboard.component.html',
})
export class DashboardComponent implements OnDestroy, OnChanges {

  private alive = true;

  solarValue: number;
  lightCard: CardSettings = {
    title: 'Light',
    iconClass: 'nb-lightbulb',
    type: 'primary',
  };
  rollerShadesCard: CardSettings = {
    title: 'Roller Shades',
    iconClass: 'nb-roller-shades',
    type: 'success',
  };
  wirelessAudioCard: CardSettings = {
    title: 'Wireless Audio',
    iconClass: 'nb-audio',
    type: 'info',
  };
  coffeeMakerCard: CardSettings = {
    title: 'Coffee Maker',
    iconClass: 'nb-coffee-maker',
    type: 'warning',
  };

  statusCards: string;

  commonStatusCardsSet: CardSettings[] = [
    this.lightCard,
    this.rollerShadesCard,
    this.wirelessAudioCard,
    this.coffeeMakerCard,
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
        ...this.rollerShadesCard,
        type: 'primary',
      },
      {
        ...this.wirelessAudioCard,
        type: 'danger',
      },
      {
        ...this.coffeeMakerCard,
        type: 'info',
      },
    ],
    dark: this.commonStatusCardsSet,
  };

  public luminocidad: Observable<any>;
  public humedad: Observable<any>;
  public temperatura: Observable<any>;
  public movimiento: Observable<any>;

  public nodoMAC: Observable<any>;
  public nodoMACValue: String;

  constructor(private themeService: NbThemeService,
              private solarService: SolarData,
              private firedbService: FiredbService,
              private nbMenuService: NbMenuService
  ) {
    this.nbMenuService.getSelectedItem().subscribe( nbMenuBag => {
      console.log(nbMenuBag);

    });

    this.nodoMAC = this.firedbService.getNodoMAC();

    this.nodoMAC
      .pipe(takeWhile(() => this.alive))
      .subscribe(value => {
        this.nodoMACValue = value;
        this.loadValues();
    });

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

  ngOnDestroy() {
    this.alive = false;
  }

  ngOnChanges() {
    console.log("cambio");
  }

  loadValues(){
    this.luminocidad = this.firedbService.getLuminocidad(this.nodoMACValue);
    this.humedad = this.firedbService.getHumedad(this.nodoMACValue);
    this.temperatura = this.firedbService.getTemperatura(this.nodoMACValue);
    this.movimiento = this.firedbService.getMovimiento(this.nodoMACValue);
  }
}

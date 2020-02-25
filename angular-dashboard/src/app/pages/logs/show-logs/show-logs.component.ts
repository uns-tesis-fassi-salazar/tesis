import { Component, OnInit, OnDestroy } from '@angular/core';
import { Observable } from 'rxjs';
import { Log } from '../../../models';
import { LogService, NodoService, AulaService } from '../../../services';
import { ActivatedRoute } from '@angular/router';
import { takeWhile, map } from 'rxjs/operators';

@Component({
  selector: 'show-logs',
  templateUrl: './show-logs.component.html',
  styleUrls: ['./show-logs.component.scss']
})
export class ShowLogsComponent implements OnInit, OnDestroy {
  logs: Log[];
  nodoMac: string = '';
  alive = true;
  isScrolledOnTop = true;
  logsFromFirebase: Log[];
  logsMinLength = 500;
  logsCurrentLength: number;
  lengthButtonMsg = "Mostrar todos";

  constructor(private logService: LogService,
    private route: ActivatedRoute,
  ) { }

  ngOnInit() {
    this.logsFromFirebase = [];
    this.logsCurrentLength = this.logsMinLength;
    this.route.paramMap
      .pipe(
        takeWhile(() => this.alive),
        map(paramMap => paramMap.get('nodoMac'))
      )
      .subscribe(nodoMac => {
        this.nodoMac = ' - ' + nodoMac;
        this.logService.getLogsByMac(nodoMac).pipe(takeWhile(() => this.alive))
          .subscribe(logs => {
            logs = logs.sort((a, b) => {
              if (a.secuencial < b.secuencial) return 1;
              else if (a.secuencial == b.secuencial) return 0;
              else return -1;
            });
            this.logsFromFirebase = logs;
            if(this.logsMinLength == this.logsCurrentLength)
              this.logs = this.logsFromFirebase.slice(0, this.logsCurrentLength);
          });
      })

  }

  ngOnDestroy() {
    this.alive = false;
  }

  changeLogsLength() {
    if (this.logsCurrentLength == this.logsMinLength) {
      this.logsCurrentLength = this.logsFromFirebase.length;
      this.logs = this.logsFromFirebase;
    } else {
      this.logsCurrentLength = this.logsMinLength;
      this.logs = this.logsFromFirebase.slice(0, this.logsCurrentLength);
    }
    this.lengthButtonMsg = this.lengthButtonMsg == 'Mostrar todos' ? 'Mostrar menos': 'Mostrar todos';
  }

  onScroll(e) {
    const out = e.srcElement;
    console.log('-----------')
    console.log(e);
    console.log(out.scrollTop);
    console.log(out.scrollHeight);
    console.log(out.clientHeight);
    this.isScrolledOnTop = out.scrollTop == 0;
    console.log(out.scrollHeight - out.clientHeight <= out.scrollTop + 1);
  }

}

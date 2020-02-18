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
  isScrolledToBottom = true;

  constructor(private logService: LogService,
    private route: ActivatedRoute,
  ) { }

  ngOnInit() {
    this.route.paramMap
      .pipe(
        takeWhile(() => this.alive),
        map(paramMap => paramMap.get('nodoMac'))
      )
      .subscribe(nodoMac => {
        this.nodoMac = ' - ' + nodoMac;
        this.logService.getLogsByMac(nodoMac).pipe(takeWhile(() => this.alive))
          .subscribe(logs => {
            this.logs = logs.sort((a, b) => {
              if (a.secuencial < b.secuencial) return 1;
              else if (a.secuencial == b.secuencial) return 0;
              else return -1;
            });
            // if (this.isScrolledToBottom)
            //   this.updateScroll()
          });
      })

  }

  ngOnDestroy() {
    this.alive = false;
  }


  updateScroll() {
    const element = document.getElementById("srollViewport");
    element.scrollTop = element.scrollHeight;
  }

  onScroll(e) {
    const out = e.srcElement;
    this.isScrolledToBottom = out.scrollHeight - out.clientHeight <= out.scrollTop + 1;
  }

}

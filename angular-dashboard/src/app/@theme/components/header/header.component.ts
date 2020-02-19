import { Component, OnDestroy, OnInit } from '@angular/core';
import { NbMediaBreakpointsService, NbMenuService, NbSidebarService, NbThemeService } from '@nebular/theme';

import { UserData } from '../../../@core/data/users';
import { LayoutService } from '../../../@core/utils';
import { map, takeUntil, filter } from 'rxjs/operators';
import { Subject } from 'rxjs';
import { AuthService } from '../../../services/auth.service';

@Component({
  selector: 'ngx-header',
  styleUrls: ['./header.component.scss'],
  templateUrl: './header.component.html',
})
export class HeaderComponent implements OnInit, OnDestroy {

  private destroy$: Subject<void> = new Subject<void>();
  userPictureOnly: boolean = false;
  isLessThanLg: boolean = false;
  isLessThanXl: boolean = false;
  user: any;

  themes = [
    {
      value: 'default',
      name: 'Light',
    },
    {
      value: 'dark',
      name: 'Dark',
    },
    {
      value: 'cosmic',
      name: 'Cosmic',
    },
    {
      value: 'corporate',
      name: 'Corporate',
    },
  ];

  currentTheme = 'default';

  // userMenu = [ { title: 'Profile' }, { title: 'Log out' } ];
  userMenu = [{ title: 'Salir' }];

  constructor(private sidebarService: NbSidebarService,
    private menuService: NbMenuService,
    private themeService: NbThemeService,
    private userService: UserData,
    private layoutService: LayoutService,
    private breakpointService: NbMediaBreakpointsService,
    private authService: AuthService) {
  }

  ngOnInit() {
    this.currentTheme = this.themeService.currentTheme;

    this.authService.currentUser
      .pipe(takeUntil(this.destroy$))
      .subscribe((user: any) => this.user = user);
    
    const { lg, xl } = this.breakpointService.getBreakpointsMap();
    this.themeService.onMediaQueryChange()
      .pipe(
        takeUntil(this.destroy$),
      )
      .subscribe( ([, currentBreakpoint]) => {
        this.userPictureOnly = currentBreakpoint.width < xl;;
        this.isLessThanXl = currentBreakpoint.width < xl;
        this.isLessThanLg = currentBreakpoint.width < lg;
      });
    
    this.themeService.onThemeChange()
      .pipe(
        map(({ name }) => name),
        takeUntil(this.destroy$),
      )
      .subscribe(themeName => this.currentTheme = themeName);

    this.menuService.onItemClick()
      .pipe(
        filter(({ tag }) => tag === 'user-context-menu'),
        map(({ item: { title } }) => title),
        takeUntil(this.destroy$),
      )
      .subscribe(title => {
        if(title === 'Salir'){// Salir
          this.authService.signOut();
        } 
      });

      this.menuService.onItemClick()
      .pipe(
        filter(({ tag }) => tag === 'sidebarMenu'),
        map(({ item, tag }) => item),
        takeUntil(this.destroy$),
      )
      .subscribe( nbMenuItem => {
        if(this.isLessThanXl){
          this.sidebarService.collapse('menu-sidebar');
          this.menuService.collapseAll('sidebarMenu');
        }
      });
  }

  ngOnDestroy() {
    this.destroy$.next();
    this.destroy$.complete();
  }

  changeTheme(themeName: string) {
    this.themeService.changeTheme(themeName);
  }

  toggleSidebar(): boolean {
    this.sidebarService.toggle(true, 'menu-sidebar');
    this.layoutService.changeLayoutSize();

    return false;
  }

  navigateHome() {
    this.menuService.navigateHome();
    return false;
  }
}

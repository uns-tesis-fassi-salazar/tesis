import { NbMenuItem } from '@nebular/theme';

export const MENU_ITEMS: NbMenuItem[] = [
  // {
  //   title: 'Dashboard',
  //   icon: 'home-outline',
  //   link: '/pages/dashboard',
  //   // home: true,
  // },
  {
    title: 'Universidad Nacional del Sur',
    group: true,
  },
  {
    title: 'DCIC2',
    icon: 'keypad-outline',
    children: [
      {
        title: 'Aula 1',
        link: '/',
      },
    ],
  },
  {
    title: 'Miscellaneous',
    icon: 'shuffle-2-outline',
    children: [
      {
        title: '404',
        link: '/pages/miscellaneous/404',
      },
    ],
  },
];

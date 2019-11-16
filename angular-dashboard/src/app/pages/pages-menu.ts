import { NbMenuItem } from '@nebular/theme';

export const MENU_ITEMS: NbMenuItem[] = [
  {
    title: 'IoT Dashboard',
    icon: 'home-outline',
    link: '/pages/iot-dashboard',
  },
  {
    title: 'Universidad Nacional del Sur',
    group: true,
  },
  {
    title: 'DCIC',
    icon: 'keypad-outline',
    children: [
      {
        title: 'Aula 1',
        link: '/',
      },
      {
        title: 'Aula 2',
        link: '/pages/forms/layouts',
      },
      {
        title: 'Aula 3',
        link: '/pages/forms/buttons',
      },
      {
        title: 'Aula 4',
        link: '/pages/forms/datepicker',
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
  {
    title: 'Auth',
    icon: 'lock-outline',
    children: [
      {
        title: 'Login',
        link: '/auth/login',
      },
      {
        title: 'Register',
        link: '/auth/register',
      },
      {
        title: 'Request Password',
        link: '/auth/request-password',
      },
      {
        title: 'Reset Password',
        link: '/auth/reset-password',
      },
    ],
  },
];

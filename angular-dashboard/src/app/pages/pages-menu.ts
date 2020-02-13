import { NbMenuItem } from '@nebular/theme';

export const MENU_ITEMS: NbMenuItem[] = [
  {
    title: 'Inicio',
    icon: 'home-outline',
    link: '/inicio',
    home: true,
  },
  {
    title: 'Edificios y aulas',
    icon: 'cube-outline',
    link: '/edificios',
  },
  {
    title: 'Configuración',
    icon: 'settings-2-outline',
    children: [
      {
        title: 'Comandos infrarrojos',
        link: '/configuracion/comandos-ir',
      },
      {
        title: 'Firmware',
        link: '/configuracion/firmware',
      },
    ]
  },
  {
    title: 'Logs',
    icon: 'file-text-outline',
    link: '/logs',
  },
  {
    title: 'Edificios',
    group: true,
  }
];

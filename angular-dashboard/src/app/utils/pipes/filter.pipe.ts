import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
    name: 'filterByProperty'
})
export class FilterByPropertyPipe implements PipeTransform {
    transform(items: any[], porpertyName: string , searchText: string): any[] {
        if (!items) return [];
        if (!searchText || !porpertyName) return items;
        searchText = searchText.toLowerCase();
        return items.filter(it => {
            return it[porpertyName].toLowerCase().includes(searchText);
        });
    }
}
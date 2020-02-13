import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
    name: 'filterByProperty'
})
export class FilterByPropertiesPipe implements PipeTransform {
    transform(items: any[], propertiesName: string[], searchText: string): any[] {
        if (!items) return [];
        if (!searchText || !propertiesName) return items;
        searchText = searchText.toLowerCase();
        return items.filter(it => {
            let contieneAlgo = false;
            for (const propertyName of propertiesName) {
                if (it[propertyName].toLowerCase().includes(searchText))
                    contieneAlgo = true;
            }
            return contieneAlgo;
        });
    }
}
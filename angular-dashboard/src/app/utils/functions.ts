
/*
Agrupa los objetos de un arreglo según alguna de sus propiedades
Ejemplo
    const data = [
        { groupId: "foo", value: 1 },
        { groupId: "foo", value: 3 },
        { groupId: "foo", value: 5 },
        { groupId: "bar", value: 42 },
        { groupId: "bar", value: 1337 },
    ];

Uso
    of(data).pipe(
        map(data => groupBy(data, item => item.groupId)
    )

Resultado
    {
        "foo": [
            { "groupId": "foo", "value": 1 },
            { "groupId": "foo", "value": 3 },
            { "groupId": "foo", "value": 5 }
        ],
        "bar": [
            { "groupId": "bar", "value": 42 },
            { "groupId": "bar", "value": 1337 }
        ]
    }
*/
export function groupBy(data, keyFn) {
    const dataGrouped = data.reduce((agg, item) => {
        const group = keyFn(item);
        agg[group] = [...(agg[group] || []), item];
        return agg;
    }, {});
    return dataGrouped;
};